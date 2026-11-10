#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Parse JSON configuration file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json configJson;
        file >> configJson;
        
        // Convert JSON to config entries
        if (configJson.contains("config_entries") && configJson["config_entries"].is_array()) {
            const auto& entries = configJson["config_entries"];
            for (const auto& entry : entries) {
                if (entry.is_object() && entry.contains("key") && entry.contains("value")) {
                    std::string key = entry["key"];
                    ConfigValue value;
                    
                    // Parse value based on JSON type
                    if (entry["value"].is_string()) {
                        value = entry["value"].get<std::string>();
                    } else if (entry["value"].is_number_integer()) {
                        value = entry["value"].get<int64_t>();
                    } else if (entry["value"].is_number_unsigned()) {
                        value = entry["value"].get<uint64_t>();
                    } else if (entry["value"].is_number()) {
                        value = entry["value"].get<double>();
                    } else if (entry["value"].is_boolean()) {
                        value = entry["value"].get<bool>();
                    } else if (entry["value"].is_array()) {
                        const auto& arr = entry["value"];
                        if (!arr.empty() && arr[0].is_number_integer()) {
                            std::vector<int> intVec;
                            intVec.reserve(arr.size());
                            for (const auto& item : arr) {
                                intVec.push_back(item.get<int>());
                            }
                            value = intVec;
                        } else if (!arr.empty() && arr[0].is_number()) {
                            std::vector<double> doubleVec;
                            doubleVec.reserve(arr.size());
                            for (const auto& item : arr) {
                                doubleVec.push_back(item.get<double>());
                            }
                            value = doubleVec;
                        } else if (!arr.empty() && arr[0].is_string()) {
                            std::vector<std::string> stringVec;
                            stringVec.reserve(arr.size());
                            for (const auto& item : arr) {
                                stringVec.push_back(item.get<std::string>());
                            }
                            value = stringVec;
                        }
                    }
                    
                    ConfigSource source = ConfigSource::File;
                    if (entry.contains("source")) {
                        std::string sourceStr = entry["source"];
                        if (sourceStr == "Default") source = ConfigSource::Default;
                        else if (sourceStr == "CommandLine") source = ConfigSource::CommandLine;
                        else if (sourceStr == "Runtime") source = ConfigSource::Runtime;
                    }
                    
                    std::string description = entry.contains("description") ? entry["description"] : "";
                    pImpl->entries.emplace_back(key, value, source, description);
                }
            }
        } else {
            // For backward compatibility, try to parse old-style key=value format
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
        }
        
        return true;
    } catch (const json::exception& e) {
        // Log error and fall back to old format
        return false;
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
    
    // Create JSON structure
    json configJson;
    
    // Convert config entries to JSON
    json entriesJson = json::array();
    for (const auto& entry : pImpl->entries) {
        json entryJson;
        entryJson["key"] = entry.key;
        
        // Convert ConfigValue to JSON
        std::visit([&entryJson](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                entryJson["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                entryJson["value"] = arg;
            }
        }, entry.value);
        
        entryJson["source"] = (entry.source == ConfigSource::Default) ? "Default" :
                             (entry.source == ConfigSource::File) ? "File" :
                             (entry.source == ConfigSource::CommandLine) ? "CommandLine" : "Runtime";
        entryJson["description"] = entry.description;
        
        entriesJson.push_back(entryJson);
    }
    
    configJson["config_entries"] = entriesJson;
    
    // Write JSON to file
    file << configJson.dump(2);
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

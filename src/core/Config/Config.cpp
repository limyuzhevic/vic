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
    // Try to parse as JSON first
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        try {
            nlohmann::json jsonData = nlohmann::json::parse(jsonFile);
            // Convert JSON to config entries
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigSource source = ConfigSource::File;
                
                // Handle nested configuration (e.g., "brain.neuron_count")
                if (key.find('.') != std::string::npos) {
                    // For nested keys, we keep the full path
                    set(key, ConfigValue(it.value().dump()), source, "Configuration value");
                } else {
                    // For simple keys, convert based on JSON type
                    if (it.value().is_string()) {
                        set(key, it.value().get<std::string>(), source);
                    } else if (it.value().is_number_integer()) {
                        set(key, it.value().get<int64_t>(), source);
                    } else if (it.value().is_number_unsigned()) {
                        set(key, it.value().get<uint64_t>(), source);
                    } else if (it.value().is_number_float()) {
                        set(key, it.value().get<double>(), source);
                    } else if (it.value().is_boolean()) {
                        set(key, it.value().get<bool>(), source);
                    } else if (it.value().is_array()) {
                        // For arrays, serialize to string representation
                        set(key, it.value().dump(), source);
                    }
                }
            }
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            // Fall back to simple format if JSON parsing fails
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON object
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to appropriate JSON type
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            std::string key = entry.key;
            
            // Handle nested keys by creating nested objects
            if (key.find('.') != std::string::npos) {
                // For nested keys, create nested JSON structure
                size_t pos = key.find('.');
                std::string firstPart = key.substr(0, pos);
                std::string rest = key.substr(pos + 1);
                
                if (!jsonData.contains(firstPart)) {
                    jsonData[firstPart] = nlohmann::json::object();
                }
                
                if (!jsonData[firstPart].contains(rest)) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        jsonData[firstPart][rest] = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                        std::is_same_v<T, std::vector<double>> ||
                                        std::is_same_v<T, std::vector<std::string>>) {
                        // Parse string representation back to array
                        try {
                            jsonData[firstPart][rest] = nlohmann::json::parse(arg);
                        } catch (...) {
                            jsonData[firstPart][rest] = arg;
                        }
                    } else {
                        jsonData[firstPart][rest] = arg;
                    }
                }
            } else {
                // Handle simple keys
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                    std::is_same_v<T, std::vector<double>> ||
                                    std::is_same_v<T, std::vector<std::string>>) {
                    // Parse string representation back to array
                    try {
                        jsonData[key] = nlohmann::json::parse(arg);
                    } catch (...) {
                        jsonData[key] = arg;
                    }
                } else {
                    jsonData[key] = arg;
                }
            }
        }, entry.value);
    }
    
    // Write JSON to file with pretty formatting
    file << jsonData.dump(2);
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source, "");
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

// JSON library (nlohmann::json)
#include <nlohmann/json.hpp>

// JSON library (nlohmann::json)
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
    // Implement proper JSON parser using nlohmann::json library
    // Try to parse as JSON first (Phase 2 upgrade)
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
            return false;
        }
        
        // Parse JSON file
        auto j = json::parse(file);
        
        // Convert JSON to config entries
        for (auto& [key, value] : j.items()) {
            ConfigValue configValue;
            
            // Convert based on JSON type
            if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Determine array element type
                std::vector<ConfigValue> vec;
                if (!value.empty()) {
                    auto first_type = value[0].type();
                    if (first_type == json::value_t::number_integer) {
                        vec = value.get<std::vector<int64_t>>();
                        configValue = vec;
                    } else if (first_type == json::value_t::number_float) {
                        vec = value.get<std::vector<double>>();
                        configValue = vec;
                    } else if (first_type == json::value_t::string) {
                        vec = value.get<std::vector<std::string>>();
                        configValue = vec;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        // JSON parsing failed, fall back to simple key=value format
        NLM_LOG_INFO("JSON parsing failed for " + filepath + ", using simple format: " + std::string(e.what()));
        
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Invalid config format in " + filepath + ", line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Create JSON object
    nlohmann::json j;
    
    // Add configuration entries to JSON
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON based on type
        if (std::holds_alternative<int64_t>(entry.value)) {
            j[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            j[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            j[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            j[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int64_t>>(entry.value)) {
            j[entry.key] = std::get<std::vector<int64_t>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            j[entry.key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            j[entry.key] = std::get<std::vector<std::string>>(entry.value);
        }
        
        // Add comment if available
        if (!entry.description.empty()) {
            j[entry.key].description = entry.description;
        }
    }
    
    // Write JSON to file with indentation
    file << j.dump(2) << "\n";
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_WARNING("Attempted to set empty configuration key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added configuration key: " + key);
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
    if (key.empty()) {
        NLM_LOG_WARNING("Attempted to check empty configuration key");
        return false;
    }
    
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
    if (key.empty()) {
        NLM_LOG_WARNING("Attempted to get configuration with empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        // Numeric values convert between int/int64/double/float/etc, so a
        // value stored as double can be read as float and vice versa.
        return std::visit([](auto&& v) -> std::optional<T> {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, T>) {
                return v;
            } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
                return static_cast<T>(v);
            } else {
                NLM_LOG_WARNING("Cannot convert configuration value from type " + std::to_string(typeid(V).hash_code()) + " to " + std::to_string(typeid(T).hash_code()));
                return std::nullopt;
            }
        }, it->value);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error retrieving configuration value for key '" + key + "': " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        NLM_LOG_WARNING("Attempted to get configuration with empty key, using default");
        return defaultValue;
    }
    
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    NLM_LOG_DEBUG("Using default configuration value for key: " + key);
    return defaultValue;
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

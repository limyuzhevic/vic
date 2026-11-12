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

// Validation function implementations
namespace {
    bool validateConfig(const std::string& key, const ConfigValue& value);
    std::string validateConfig(const std::vector<ConfigEntry>& entries);
}

// Validation function implementations
namespace {
    bool validateConfig(const std::string& key, const ConfigValue& value) {
        // Basic validation based on key patterns
        if (key.empty()) return false;
        
        // Check for configuration validation rules
        if (key.find("neuron_count") != std::string::npos) {
            if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<uint64_t>(value)) {
                return false;
            }
            auto neuronCount = std::holds_alternative<int64_t>(value) 
                ? std::get<int64_t>(value) 
                : std::get<uint64_t>(value);
            if (neuronCount < 1 || neuronCount > 1000000) {
                return false;
            }
        }
        else if (key.find("synapse_density") != std::string::npos) {
            if (!std::holds_alternative<double>(value)) return false;
            auto density = std::get<double>(value);
            if (density < 0.0 || density > 1.0) {
                return false;
            }
        }
        else if (key.find("connection_probability") != std::string::npos) {
            if (!std::holds_alternative<double>(value)) return false;
            auto prob = std::get<double>(value);
            if (prob < 0.0 || prob > 1.0) {
                return false;
            }
        }
        
        return true;
    }
    
    std::string validateConfig(const std::vector<ConfigEntry>& entries) {
        std::string errors;
        size_t errorCount = 0;
        
        for (const auto& entry : entries) {
            if (!validateConfig(entry.key, entry.value)) {
                if (errorCount > 0) errors += ", ";
                errors += "Invalid value for key: " + entry.key;
                errorCount++;
                if (errorCount >= 10) { // Limit error count
                    errors += ", and " + std::to_string(entries.size() - errorCount) + " more errors...";
                    break;
                }
            }
        }
        
        return errors;
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
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&](const auto& value) {
                // Convert ConfigValue to nlohmann::json
                using T = std::decay_t<decltype(value)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, int>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = value;
                } else {
                    // Fallback for unknown types
                    jsonData[entry.key] = "UNKNOWN_TYPE";
                }
            }, entry.value);
        }
        
        // Write JSON with pretty formatting
        file << jsonData.dump(2); // 2-space indentation
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
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

// Validation method
std::string Config::validate() const {
    return ::validateConfig(pImpl->entries);
}

// Add configuration set with validation
default bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    bool isValid = ::validateConfig(key, value);
    if (!isValid) {
        NLM_LOG_WARNING("Invalid configuration value for key: " + key);
    }
    // (Original set method continues here)
}

} // namespace nlm

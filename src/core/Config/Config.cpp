#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Helper functions for JSON/YAML support
namespace {
    // Convert ConfigValue to JSON
    nlohmann::json configValueToJson(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                return static_cast<int64_t>(arg);
            } else if constexpr (std::is_floating_point_v<T>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return arg;
            } else {
                throw std::runtime_error("Unsupported ConfigValue type");
            }
        }, value);
    }

    // Convert JSON to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& json) {
        if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_number_integer()) {
            return json.get<int64_t>();
        } else if (json.is_number_unsigned()) {
            // For unsigned, use the largest integer type available
            return json.get<uint64_t>();
        } else if (json.is_number()) {
            return json.get<double>();
        } else if (json.is_array()) {
            // Determine array type based on content
            if (!json.empty()) {
                if (json[0].is_string()) {
                    return json.get<std::vector<std::string>>();
                } else if (json[0].is_number_integer()) {
                    return json.get<std::vector<int64_t>>();
                } else {
                    return json.get<std::vector<double>>();
                }
            }
            return std::vector<double>();
        } else {
            throw std::runtime_error("Unsupported JSON type");
        }
    }

    // Parse string value to appropriate type
    void parseStringToConfig(ConfigValue& value, const std::string& str) {
        // Try boolean
        if (str == "true") {
            value = true;
            return;
        } else if (str == "false") {
            value = false;
            return;
        }

        // Try integer
        try {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                value = intVal;
                return;
            }
        } catch (...) {}

        // Try unsigned integer
        try {
            size_t pos;
            uint64_t uintVal = std::stoull(str, &pos);
            if (pos == str.size()) {
                value = uintVal;
                return;
            }
        } catch (...) {}

        // Try double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                value = doubleVal;
                return;
            }
        } catch (...) {}

        // Try comma-separated list
        size_t commaPos = str.find(',');
        if (commaPos != std::string::npos) {
            std::vector<double> list;
            std::stringstream ss(str);
            std::string item;
            while (std::getline(ss, item, ',')) {
                try {
                    list.push_back(std::stod(item));
                } catch (...) {
                    // Skip invalid items
                }
            }
            if (!list.empty()) {
                value = list;
                return;
            }
        }

        // Default to string
        value = str;
    }

    // Helper to detect file type
    std::string detectFileType(const std::string& filepath) {
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
}

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

    // Detect file type by extension
    std::string ext = detectFileType(filepath);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        // For now, treat YAML as JSON (yaml-cpp not available)
        // In the future, add yaml-cpp support
        return loadFromSimpleFormat(filepath);
    } else {
        // Fall back to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json jsonData;
        file >> jsonData;

        // Clear existing entries
        pImpl->entries.clear();

        // Process JSON object
        for (auto& [key, value] : jsonData.items()) {
            try {
                ConfigValue configValue = jsonToConfigValue(value);
                set(key, configValue, ConfigSource::File);
            } catch (const std::exception& e) {
                // Log error but continue with other entries
                std::cerr << "Warning: Failed to parse config value for key '" 
                          << key << "': " << e.what() << std::endl;
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to parse JSON config file '" 
                  << filepath << "': " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

            // Convert string value to appropriate type
            ConfigValue configValue;
            parseStringToConfig(configValue, value);
            
            set(key, configValue, ConfigSource::File);
        }
    }

    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Create JSON representation
    nlohmann::json jsonData;

    for (const auto& entry : pImpl->entries) {
        try {
            jsonData[entry.key] = configValueToJson(entry.value);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to serialize config value for key '" 
                      << entry.key << "': " << e.what() << std::endl;
        }
    }

    // Write JSON to file with indentation for readability
    file << std::setw(2) << jsonData << std::endl;

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
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                oss << arg;
            } else if constexpr (std::is_floating_point_v<T>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
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
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else if constexpr (std::is_same_v<V, std::string> && std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<V, bool> && std::is_same_v<T, bool>) {
            return v;
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
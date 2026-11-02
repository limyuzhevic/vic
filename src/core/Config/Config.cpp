#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <numeric>  // For std::numeric_limits

// Include these only if they're available
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace nlm {

// Helper functions for JSON/YAML conversion

// Convert ConfigValue to JSON value
nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || 
                      std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(v);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(v);
        } else {
            throw std::runtime_error("Unsupported ConfigValue type for JSON conversion");
        }
    }, value);
}

// Convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_number_integer()) {
        // Try to preserve the original type (int64_t preferred for portability)
        if (j.get<int64_t>() >= std::numeric_limits<int>::min() && 
            j.get<int64_t>() <= std::numeric_limits<int>::max()) {
            return j.get<int>();
        } else {
            return j.get<int64_t>();
        }
    } else if (j.is_number_unsigned()) {
        if (j.get<uint64_t>() <= std::numeric_limits<uint32_t>::max()) {
            return j.get<uint32_t>();
        } else {
            return j.get<uint64_t>();
        }
    } else if (j.is_number_float()) {
        if (std::abs(j.get<float>() - j.get<double>()) < 1e-6f) {
            return j.get<float>();
        } else {
            return j.get<double>();
        }
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            const auto& firstElement = j[0];
            if (firstElement.is_number_integer()) {
                std::vector<int> vec = j.get<std::vector<int>>();
                return vec;
            } else if (firstElement.is_number_float()) {
                std::vector<double> vec = j.get<std::vector<double>>();
                return vec;
            } else if (firstElement.is_string()) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                return vec;
            }
        }
        // Default to empty vector
        return std::vector<int>{};
    } else {
        throw std::runtime_error("Unsupported JSON type for ConfigValue conversion");
    }
}

#ifdef HAVE_YAML_CPP
// Convert ConfigValue to YAML node
YAML::Node configValueToYaml(const ConfigValue& value) {
    YAML::Node node;
    std::visit([&](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || 
                      std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>) {
            node = v;
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            node = v;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : v) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : v) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : v) {
                node.push_back(item);
            }
        } else {
            throw std::runtime_error("Unsupported ConfigValue type for YAML conversion");
        }
    }, value);
    return node;
}

// Convert YAML node to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.IsInteger()) {
            int64_t val = node.as<int64_t>();
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                return static_cast<int>(val);
            } else {
                return val;
            }
        } else if (node.IsFloat()) {
            double val = node.as<double>();
            if (std::abs(static_cast<float>(val) - val) < 1e-6f) {
                return static_cast<float>(val);
            } else {
                return val;
            }
        } else if (node.IsBoolean()) {
            return node.as<bool>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (!node.empty()) {
            const YAML::Node& firstElement = node[0];
            if (firstElement.IsInteger()) {
                std::vector<int> vec;
                for (const auto& item : node) {
                    vec.push_back(item.as<int>());
                }
                return vec;
            } else if (firstElement.IsFloat()) {
                std::vector<double> vec;
                for (const auto& item : node) {
                    vec.push_back(item.as<double>());
                }
                return vec;
            } else if (firstElement.IsScalar()) {
                std::vector<std::string> vec;
                for (const auto& item : node) {
                    vec.push_back(item.as<std::string>());
                }
                return vec;
            }
        }
        // Default to empty vector
        return std::vector<int>{};
    } else {
        throw std::runtime_error("Unsupported YAML type for ConfigValue conversion");
    }
}
#endif

// Helper function to detect file format
std::string detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "unknown";
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.close();
    
    // Check for JSON format (starts with { or [)
    if (!firstLine.empty() && (firstLine[0] == '{' || firstLine[0] == '[')) {
        return "json";
    }
    // Check for YAML format (common indicators)
    else if (!firstLine.empty() && (
        firstLine.find(":") != std::string::npos || 
        firstLine.find("-") != std::string::npos ||
        firstLine.find("[") != std::string::npos ||
        firstLine.find("{") != std::string::npos
    )) {
        return "yaml";
    }
    // Default to simple key=value format
    else {
        return "simple";
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Clear existing entries
    clear();
    
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            // Load as JSON
            auto j = nlohmann::json::parse(file);
            for (const auto& [key, value] : j.items()) {
                try {
                    ConfigValue configValue = jsonToConfigValue(value);
                    set(key, configValue, ConfigSource::File);
                    
                    // Try to get description from JSON if available
                    if (value.contains("_description")) {
                        std::string desc = value["_description"].get<std::string>();
                        auto it = std::find_if(pImpl->entries.rbegin(), pImpl->entries.rend(),
                                            [&key](const ConfigEntry& e) { return e.key == key; });
                        if (it != pImpl->entries.rend()) {
                            it->description = desc;
                        }
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse JSON value for key '" + key + "': " + std::string(e.what()));
                }
            }
        } 
#ifdef HAVE_YAML_CPP
        else if (format == "yaml") {
            // Load as YAML
            auto y = YAML::Load(file);
            for (const auto& node : y) {
                std::string key = node.first.as<std::string>();
                try {
                    ConfigValue configValue = yamlToConfigValue(node.second);
                    set(key, configValue, ConfigSource::File);
                    
                    // Try to get description from YAML comment if available
                    // YAML doesn't have direct comment support, but we could parse it
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse YAML value for key '" + key + "': " + std::string(e.what()));
                }
            }
        }
#endif
        else {
            // Load as simple key=value format
            std::string line;
            size_t lineNum = 0;
            while (std::getline(file, line)) {
                lineNum++;
                line = trim(line);
                
                // Skip empty lines and comments
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
                    
                    // Try to convert to appropriate type
                    set(key, value, ConfigSource::File);
                } else {
                    NLM_LOG_WARNING("Skipping invalid line " + std::to_string(lineNum) + " in config file: " + line);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse config file '" + filepath + "': " + std::string(e.what()));
        return false;
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Use JSON format by default (most portable and readable)
    auto j = nlohmann::json();
    
    for (const auto& entry : pImpl->entries) {
        try {
            j[entry.key] = configValueToJson(entry.value);
            
            // Add description as metadata if available
            if (!entry.description.empty()) {
                j[entry.key]["_description"] = entry.description;
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to serialize config value for key '" + entry.key + "': " + std::string(e.what()));
            // Store as string as fallback
            j[entry.key] = entry.key + "=serialization_error";
        }
    }
    
    // Write JSON with indentation for readability
    file << j.dump(2);
    
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
template std::optional<uint32_t> Config::get<uint32_t>(const std::string&) const;
template std::optional<int8_t> Config::get<int8_t>(const std::string&) const;
template std::optional<int16_t> Config::get<int16_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template uint32_t Config::getOr<uint32_t>(const std::string&, const uint32_t&) const;
template int8_t Config::getOr<int8_t>(const std::string&, const int8_t&) const;
template int16_t Config::getOr<int16_t>(const std::string&, const int16_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm

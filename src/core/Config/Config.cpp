#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <system_error>
#include <numeric>

// JSON support
#ifdef NLM_ENABLE_JSON
#include <nlohmann/json.hpp>
#endif

// YAML support
#ifdef NLM_ENABLE_YAML
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

// Helper function to convert ConfigValue to nlohmann::json
#ifdef NLM_ENABLE_JSON
static nlohmann::json toJson(const ConfigValue& value) {
    nlohmann::json result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        } else if constexpr (std::is_arithmetic_v<T>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        }
    }, value);
    return result;
}

// Helper function to convert nlohmann::json to ConfigValue
static ConfigValue fromJson(const nlohmann::json& json) {
    ConfigValue result;
    
    if (json.is_string()) {
        result = json.get<std::string>();
    } else if (json.is_number_integer()) {
        if (json.is_signed_integer()) {
            // Try to preserve int64_t for large numbers
            int64_t val = json.get<int64_t>();
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                result = static_cast<int>(val);
            } else {
                result = val;
            }
        } else {
            result = json.get<uint64_t>();
        }
    } else if (json.is_number_float()) {
        result = json.get<double>();
    } else if (json.is_boolean()) {
        result = json.get<bool>();
    } else if (json.is_array()) {
        // Determine array type
        if (json.empty()) {
            result = std::vector<std::string>{};
        } else {
            // Try to infer element type
            if (json.front().is_string()) {
                std::vector<std::string> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                result = vec;
            } else if (json.front().is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<int>());
                }
                result = vec;
            } else if (json.front().is_number_float()) {
                std::vector<double> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                result = vec;
            } else {
                // Default to string vector
                std::vector<std::string> vec;
                vec.reserve(json.size());
                for (const auto& item : json) {
                    vec.push_back(item.dump());
                }
                result = vec;
            }
        }
    } else {
        // Default to string
        result = json.get<std::string>();
    }
    
    return result;
}
#endif

#ifdef NLM_ENABLE_YAML
// Helper function to convert YAML::Node to ConfigValue
static ConfigValue fromYaml(const YAML::Node& node) {
    ConfigValue result;
    
    if (node.IsScalar()) {
        const std::string& value = node.Scalar();
        
        // Try to parse as boolean
        if (value == "true" || value == "false") {
            result = value == "true";
        }
        // Try to parse as integer
        else if (std::all_of(value.begin(), value.end(), ::isdigit) ||
                 (value.front() == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
            if (value.find('.') == std::string::npos) {
                // Integer
                if (value.find('+') != std::string::npos || value.find('-') != std::string::npos) {
                    result = std::stoll(value);
                } else {
                    result = std::stoul(value);
                }
            } else {
                // Double
                result = std::stod(value);
            }
        }
        // Try to parse as double (including scientific notation)
        else {
            try {
                size_t pos;
                double d = std::stod(value, &pos);
                if (pos == value.size()) {
                    result = d;
                } else {
                    result = value;
                }
            } catch (...) {
                result = value;
            }
        }
    } else if (node.IsSequence()) {
        // Determine array type
        if (node.size() == 0) {
            result = std::vector<std::string>{};
        } else {
            // Check if all elements are strings
            bool allStrings = std::all_of(node.begin(), node.end(),
                [](const YAML::Node& n) { return n.IsScalar() && !n.Scalar().empty(); });
            
            if (allStrings) {
                std::vector<std::string> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.Scalar());
                }
                result = vec;
            } else {
                // Default to string vector
                std::vector<std::string> vec;
                vec.reserve(node.size());
                for (const auto& item : node) {
                    vec.push_back(item.Scalar());
                }
                result = vec;
            }
        }
    } else {
        // Default to string representation
        result = node.as<std::string>();
    }
    
    return result;
}

// Helper function to convert ConfigValue to YAML
static YAML::Node toYaml(const ConfigValue& value) {
    YAML::Node result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                result.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                result.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                result.push_back(item);
            }
        } else if constexpr (std::is_arithmetic_v<T>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        }
    }, value);
    
    return result;
}
#endif

// Helper function to detect file format
Config::ConfigFormat Config::detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return ConfigFormat::KEY_VALUE;
    }
    
    // Read first few lines
    std::string line;
    std::getline(file, line);
    
    // Check for JSON format (starts with { or [)
    if (!line.empty()) {
        char first = line.front();
        if (first == '{' || first == '[') {
            return ConfigFormat::JSON;
        }
        
        // Check for YAML format (contains colon)
        if (line.find(':') != std::string::npos) {
            return ConfigFormat::YAML;
        }
    }
    
    // Default to key=value format
    return ConfigFormat::KEY_VALUE;
}

bool Config::loadFromJsonFile(const std::string& filepath) {
#ifdef NLM_ENABLE_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        return loadFromJsonString(jsonData.dump());
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::loadFromYamlFile(const std::string& filepath) {
#ifdef NLM_ENABLE_YAML
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        return loadFromYamlString(yamlData);
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format and load accordingly
    ConfigFormat format = detectFormat(filepath);
    
    switch (format) {
        case ConfigFormat::JSON:
            return loadFromJsonFile(filepath);
        case ConfigFormat::YAML:
            return loadFromYamlFile(filepath);
        case ConfigFormat::KEY_VALUE:
            return loadFromKeyValueFile(filepath);
        default:
            return loadFromKeyValueFile(filepath);
    }
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
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

bool Config::saveToFile(const std::string& filepath, bool useJson, bool useYaml) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (useJson) {
        nlohmann::json jsonData;
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                jsonData[entry.key] = {{ "value", toJson(entry.value) }, { "description", entry.description }};
            } else {
                jsonData[entry.key] = toJson(entry.value);
            }
        }
        file << jsonData.dump(2);
    } else if (useYaml) {
#ifdef NLM_ENABLE_YAML
        YAML::Emitter yamlData;
        yamlData << YAML::BeginMap;
        for (const auto& entry : pImpl->entries) {
            yamlData << YAML::Key << entry.key;
            if (!entry.description.empty()) {
                yamlData << YAML::Value << YAML::BeginMap;
                yamlData << YAML::Key << "value" << toJson(entry.value);
                yamlData << YAML::Key << "description" << entry.description;
                yamlData << YAML::EndMap;
            } else {
                yamlData << YAML::Value << toJson(entry.value);
            }
        }
        yamlData << YAML::EndMap;
        file << yamlData.c_str();
#else
        // YAML not enabled, fall back to key=value
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
#endif
    } else {
        // Default to key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    }
    
    return true;
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    return saveToFile(filepath, true, false);
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    return saveToFile(filepath, false, true);
}

bool Config::loadFromJsonString(const std::string& jsonString) {
#ifdef NLM_ENABLE_JSON
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        clear();
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigValue configValue = fromJson(value);
            set(key, configValue, ConfigSource::File);
            
            // Extract description if present
            if (value.is_object() && value.contains("description")) {
                std::string description = value["description"].get<std::string>();
                // Update the entry with description
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != pImpl->entries.end()) {
                    it->description = description;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::loadFromYamlString(const std::string& yamlString) {
#ifdef NLM_ENABLE_YAML
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        return loadFromYamlString(yamlData);
    } catch (const std::exception& e) {
        return false;
    }
#else
    return false;
#endif
}

bool Config::loadFromYamlString(const YAML::Node& yamlData) {
#ifdef NLM_ENABLE_YAML
    clear();
    
    if (yamlData.IsMap()) {
        for (const auto& pair : yamlData) {
            std::string key = pair.first.as<std::string>();
            YAML::Node value = pair.second;
            
            ConfigValue configValue = fromYaml(value);
            set(key, configValue, ConfigSource::File);
            
            // Extract description if present
            if (value.IsMap() && value["description"]) {
                std::string description = value["description"].as<std::string>();
                // Update the entry with description
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != pImpl->entries.end()) {
                    it->description = description;
                }
            }
        }
    }
    
    return true;
#else
    return false;
#endif
}

std::string Config::toJson() const {
#ifdef NLM_ENABLE_JSON
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            jsonData[entry.key] = {{ "value", toJson(entry.value) }, { "description", entry.description }};
        } else {
            jsonData[entry.key] = toJson(entry.value);
        }
    }
    
    return jsonData.dump(2);
#else
    return "{\"error\": \"JSON support not enabled\"}";
#endif
}

std::string Config::toYaml() const {
#ifdef NLM_ENABLE_YAML
    YAML::Emitter yamlData;
    yamlData << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        yamlData << YAML::Key << entry.key;
        if (!entry.description.empty()) {
            yamlData << YAML::Value << YAML::BeginMap;
            yamlData << YAML::Key << "value" << toJson(entry.value);
            yamlData << YAML::Key << "description" << entry.description;
            yamlData << YAML::EndMap;
        } else {
            yamlData << YAML::Value << toJson(entry.value);
        }
    }
    
    yamlData << YAML::EndMap;
    return yamlData.c_str();
#else
    return "error: YAML support not enabled";
#endif
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

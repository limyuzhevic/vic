#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// JSON support
#ifdef __has_include
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#define HAS_NLOHMANN_JSON 1
#endif
#endif

// YAML support
#ifdef __has_include
#if __has_include(<yaml-cpp/yaml.h>)
#include <yaml-cpp/yaml.h>
#define HAS_YAML_CPP 1
#endif
#endif

namespace nlm {

// JSON helper functions
#ifdef HAS_NLOHMANN_JSON
nlohmann::json configEntryToJson(const ConfigEntry& entry) {
    nlohmann::json j;
    j["key"] = entry.key;
    
    // Convert ConfigValue to JSON
    std::visit([&j](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, double>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j["value"] = value;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j["value"] = value;
        }
    }, entry.value);
    
    j["source"] = static_cast<int>(entry.source);
    j["description"] = entry.description;
    return j;
}

ConfigEntry jsonToConfigEntry(const nlohmann::json& j) {
    ConfigEntry entry;
    if (j.contains("key")) entry.key = j["key"].get<std::string>();
    if (j.contains("source")) entry.source = static_cast<ConfigSource>(j["source"].get<int>());
    if (j.contains("description")) entry.description = j["description"].get<std::string>();
    
    // Convert JSON value back to ConfigValue
    if (j.contains("value")) {
        const auto& val = j["value"];
        if (val.is_number_integer()) {
            // Try int first, then int64_t
            try { entry.value = val.get<int>(); }
            catch (...) {
                entry.value = val.get<int64_t>();
            }
        } else if (val.is_number_float()) {
            entry.value = val.get<double>();
        } else if (val.is_boolean()) {
            entry.value = val.get<bool>();
        } else if (val.is_string()) {
            entry.value = val.get<std::string>();
        } else if (val.is_array()) {
            // Try to infer vector type from first element
            if (!val.empty()) {
                const auto& first = val[0];
                if (first.is_number_integer()) {
                    entry.value = val.get<std::vector<int>>();
                } else if (first.is_number_float()) {
                    entry.value = val.get<std::vector<double>>();
                } else if (first.is_string()) {
                    entry.value = val.get<std::vector<std::string>>();
                }
            }
        }
    }
    
    return entry;
}
#endif

// YAML helper functions
#ifdef HAS_YAML_CPP
YAML::Node configEntryToYaml(const ConfigEntry& entry) {
    YAML::Node node;
    node["key"] = entry.key;
    
    // Convert ConfigValue to YAML
    std::visit([&node](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, double>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            node["value"] = value;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            node["value"] = value;
        }
    }, entry.value);
    
    node["source"] = static_cast<int>(entry.source);
    node["description"] = entry.description;
    return node;
}

ConfigEntry yamlToConfigEntry(const YAML::Node& node) {
    ConfigEntry entry;
    if (node["key"]) entry.key = node["key"].as<std::string>();
    if (node["source"]) entry.source = static_cast<ConfigSource>(node["source"].as<int>());
    if (node["description"]) entry.description = node["description"].as<std::string>();
    
    // Convert YAML value back to ConfigValue
    if (node["value"]) {
        const YAML::Node& val = node["value"];
        if (val.IsInteger()) {
            try { entry.value = val.as<int>(); }
            catch (...) { entry.value = val.as<int64_t>(); }
        } else if (val.IsFloat()) {
            entry.value = val.as<double>();
        } else if (val.IsBoolean()) {
            entry.value = val.as<bool>();
        } else if (val.IsScalar()) {
            entry.value = val.as<std::string>();
        } else if (val.IsSequence()) {
            // Try to infer vector type from first element
            if (!val.empty()) {
                const YAML::Node& first = val[0];
                if (first.IsInteger()) {
                    entry.value = val.as<std::vector<int>>();
                } else if (first.IsFloat()) {
                    entry.value = val.as<std::vector<double>>();
                } else if (first.IsScalar()) {
                    entry.value = val.as<std::vector<std::string>>();
                }
            }
        }
    }
    
    return entry;
}
#endif

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
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

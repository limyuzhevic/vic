#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool jsonMode = false;  // Track which parser mode is active
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Determine file format based on extension
        std::filesystem::path path(filepath);
        std::string extension = path.extension().string();
        
        if (extension == ".json") {
            pImpl->jsonMode = true;
            return loadFromJSON(filepath);
        } else if (extension == ".yaml" || extension == ".yml") {
            pImpl->jsonMode = false;
            return loadFromYAML(filepath);
        } else {
            // Fallback to simple format for backward compatibility
            pImpl->jsonMode = false;
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading config from file: ") + e.what());
        return false;
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
        clear();
        
        // Recursively parse JSON data
        parseJSONData(jsonData, ConfigSource::File);
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading JSON config: ") + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing entries
        clear();
        
        // Recursively parse YAML data
        parseYAMLData(yamlData, ConfigSource::File);
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading YAML config: ") + e.what());
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing entries for backward compatibility
    pImpl->entries.clear();
    
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

// Helper functions for JSON parsing
void Config::parseJSONData(const nlohmann::json& jsonData, ConfigSource source) {
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const nlohmann::json& value = it.value();
            setValueFromJSON(key, value, source);
        }
    }
}

void Config::parseYAMLData(const YAML::Node& yamlData, ConfigSource source) {
    if (yamlData.IsMap()) {
        for (const auto& it : yamlData) {
            std::string key = it.first.as<std::string>();
            const YAML::Node& value = it.second;
            setValueFromYAML(key, value, source);
        }
    }
}

void Config::setValueFromJSON(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_unsigned()) {
        set(key, value.get<uint64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Handle arrays based on element type
        if (!value.empty()) {
            if (value[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<std::string>());
                }
                set(key, vec, source);
            } else if (value[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<int>());
                }
                set(key, vec, source);
            } else if (value[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<double>());
                }
                set(key, vec, source);
            }
        } else {
            set(key, std::vector<std::string>(), source);
        }
    }
}

void Config::setValueFromYAML(const std::string& key, const YAML::Node& value, ConfigSource source) {
    if (value.IsScalar()) {
        std::string strValue = value.as<std::string>();
        // Try to determine type from YAML boolean/number detection
        if (strValue == "true" || strValue == "false") {
            set(key, strValue == "true", source);
        } else {
            // Try numeric conversion
            try {
                if (strValue.find('.') != std::string::npos) {
                    set(key, std::stod(strValue), source);
                } else {
                    set(key, std::stoll(strValue), source);
                }
            } catch (...) {
                set(key, strValue, source);
            }
        }
    } else if (value.IsSequence()) {
        std::vector<std::string> vec;
        for (const auto& item : value) {
            vec.push_back(item.as<std::string>());
        }
        set(key, vec, source);
    }
}

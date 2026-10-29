#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return loadFromYAML(filepath);
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
        return loadFromSimpleFormat(filepath);
    } else {
        // Try to detect format from content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear();
        file.seekg(0);
        
        if (firstLine.find("{") != std::string::npos) {
            return loadFromJSON(filepath);
        } else {
            return loadFromSimpleFormat(filepath);
        }
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
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
    
    NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            const auto& jsonVal = *it;
            
            if (jsonVal.is_string()) {
                value = jsonVal.get<std::string>();
            } else if (jsonVal.is_number_integer()) {
                value = jsonVal.get<int64_t>();
            } else if (jsonVal.is_number_float()) {
                value = jsonVal.get<double>();
            } else if (jsonVal.is_boolean()) {
                value = jsonVal.get<bool>();
            } else if (jsonVal.is_array()) {
                // Handle array - convert to appropriate vector type based on element type
                std::vector<int64_t> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& elem : jsonVal) {
                    if (elem.is_number_integer()) {
                        intArray.push_back(elem.get<int64_t>());
                    } else if (elem.is_number_float()) {
                        doubleArray.push_back(elem.get<double>());
                    } else if (elem.is_string()) {
                        stringArray.push_back(elem.get<std::string>());
                    }
                }
                
                if (!intArray.empty()) {
                    value = intArray;
                } else if (!doubleArray.empty()) {
                    value = doubleArray;
                } else if (!stringArray.empty()) {
                    value = stringArray;
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML configuration file: " + filepath);
            return false;
        }
        
        YAML::Node yamlData = YAML::Load(file);
        
        // Convert YAML to config entries
        for (const auto& it : yamlData) {
            std::string key = it.first.as<std::string>();
            
            // Convert YAML value to ConfigValue
            ConfigValue value;
            const YAML::Node& yamlVal = it.second;
            
            if (yamlVal.IsScalar()) {
                if (yamlVal.IsInteger()) {
                    value = yamlVal.as<int64_t>();
                } else if (yamlVal.IsDouble()) {
                    value = yamlVal.as<double>();
                } else if (yamlVal.IsBoolean()) {
                    value = yamlVal.as<bool>();
                } else {
                    value = yamlVal.as<std::string>();
                }
            } else if (yamlVal.IsSequence()) {
                // Handle sequence - convert to appropriate vector type
                std::vector<int64_t> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& elem : yamlVal) {
                    if (elem.IsInteger()) {
                        intArray.push_back(elem.as<int64_t>());
                    } else if (elem.IsDouble()) {
                        doubleArray.push_back(elem.as<double>());
                    } else if (elem.IsScalar()) {
                        stringArray.push_back(elem.as<std::string>());
                    }
                }
                
                if (!intArray.empty()) {
                    value = intArray;
                } else if (!doubleArray.empty()) {
                    value = doubleArray;
                } else if (!stringArray.empty()) {
                    value = stringArray;
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded YAML configuration from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML configuration file " + filepath + ": " + std::string(e.what()));
        return false;
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

// Advanced configuration methods
std::map<std::string, std::string> Config::getAllStringValues() const {
    std::map<std::string, std::string> result;
    for (const auto& entry : pImpl->entries) {
        std::visit([&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result[entry.key] = value;
            } else if constexpr (std::is_same_v<T, int>) {
                result[entry.key] = std::to_string(value);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result[entry.key] = std::to_string(value);
            } else if constexpr (std::is_same_v<T, double>) {
                result[entry.key] = std::to_string(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                result[entry.key] = value ? "true" : "false";
            }
        }, entry.value);
    }
    return result;
}

bool Config::validate(const std::string& schemaFile) const {
    // Simple validation - check for required keys
    if (schemaFile.empty()) {
        // Check for essential keys
        std::vector<std::string> essentialKeys = {"neuron_count", "region_count"};
        for (const auto& key : essentialKeys) {
            if (!has(key)) {
                NLM_LOG_ERROR("Missing required configuration key: " + key);
                return false;
            }
        }
    }
    return true;
}

bool Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
        }
        // If exists, don't override to preserve source priority
    }
    return true;
}

std::string Config::toJSON() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](const auto& value) {
            jsonData[entry.key] = value;
        }, entry.value);
    }
    
    return jsonData.dump(2); // Pretty print with 2-space indentation
}

std::string Config::toYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << YAML::Value << value;
            } else if constexpr (std::is_same_v<T, int>) {
                emitter << YAML::Value << value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << YAML::Value << value;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << YAML::Value << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << YAML::Value << value;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : value) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : value) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : value) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    
    std::stringstream stream;
    stream << emitter;
    return stream.str();
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonVal) {
    if (jsonVal.is_string()) {
        return jsonVal.get<std::string>();
    } else if (jsonVal.is_number_integer()) {
        return jsonVal.get<int64_t>();
    } else if (jsonVal.is_number_float()) {
        return jsonVal.get<double>();
    } else if (jsonVal.is_boolean()) {
        return jsonVal.get<bool>();
    } else if (jsonVal.is_array()) {
        // Try to infer array type
        std::vector<int64_t> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        for (const auto& elem : jsonVal) {
            if (elem.is_number_integer()) {
                intArray.push_back(elem.get<int64_t>());
            } else if (elem.is_number_float()) {
                doubleArray.push_back(elem.get<double>());
            } else if (elem.is_string()) {
                stringArray.push_back(elem.get<std::string>());
            }
        }
        
        if (!intArray.empty()) return intArray;
        if (!doubleArray.empty()) return doubleArray;
        if (!stringArray.empty()) return stringArray;
    }
    return std::string(); // Empty string as fallback
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& yamlVal) {
    if (yamlVal.IsScalar()) {
        if (yamlVal.IsInteger()) {
            return yamlVal.as<int64_t>();
        } else if (yamlVal.IsDouble()) {
            return yamlVal.as<double>();
        } else if (yamlVal.IsBoolean()) {
            return yamlVal.as<bool>();
        } else {
            return yamlVal.as<std::string>();
        }
    } else if (yamlVal.IsSequence()) {
        // Try to infer sequence type
        std::vector<int64_t> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        for (const auto& elem : yamlVal) {
            if (elem.IsInteger()) {
                intArray.push_back(elem.as<int64_t>());
            } else if (elem.IsDouble()) {
                doubleArray.push_back(elem.as<double>());
            } else if (elem.IsScalar()) {
                stringArray.push_back(elem.as<std::string>());
            }
        }
        
        if (!intArray.empty()) return intArray;
        if (!doubleArray.empty()) return doubleArray;
        if (!stringArray.empty()) return stringArray;
    }
    return std::string(); // Empty string as fallback
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    return std::visit([](const auto& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return val;
        }
        return nlohmann::json();
    }, value);
}

std::string Config::configValueToYAML(const ConfigValue& value) {
    std::stringstream stream;
    YAML::Emitter emitter;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            emitter << YAML::Dump(val);
        } else if constexpr (std::is_same_v<T, int>) {
            emitter << YAML::Dump(val);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            emitter << YAML::Dump(val);
        } else if constexpr (std::is_same_v<T, double>) {
            emitter << YAML::Dump(val);
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << YAML::Dump(val);
        }
    }, value);
    
    return stream.str();
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template std::optional<int> Config::getImpl<int>(const std::string&) const;
template std::optional<int64_t> Config::getImpl<int64_t>(const std::string&) const;
template std::optional<double> Config::getImpl<double>(const std::string&) const;
template std::optional<bool> Config::getImpl<bool>(const std::string&) const;
template std::optional<std::string> Config::getImpl<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
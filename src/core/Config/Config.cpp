#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <algorithm> // For std::sort
#include <unordered_set> // For set operations

// Include JSON and YAML headers if available
#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAS_YAML_CPP
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

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    if (ext == ".json") {
        // JSON parsing (using nlohmann::json)
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            success = parseJSON(jsonData);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to parse JSON file ") + filepath + ": " + e.what());
        }
    } else if (ext == ".yaml" || ext == ".yml") {
        // YAML parsing (using yaml-cpp)
        try {
            YAML::Node yamlData = YAML::LoadFile(filepath);
            success = parseYAML(yamlData);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to parse YAML file ") + filepath + ": " + e.what());
        }
    } else {
        // Fallback to simple key=value format for backward compatibility
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
        success = true;
    }
    
    file.close();
    return success;
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
    
    // Try to determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    if (ext == ".json") {
        // Save as JSON
        try {
            nlohmann::json jsonData;
            for (const auto& entry : pImpl->entries) {
                jsonData[entry.key] = std::visit([](const auto& val) -> nlohmann::json {
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
                    } else {
                        return "";
                    }
                }, entry.value);
            }
            file << jsonData.dump(2); // Pretty print with 2-space indent
            success = true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to save JSON file ") + filepath + ": " + e.what());
        }
    } else if (ext == ".yaml" || ext == ".yml") {
        // Save as YAML
        try {
            YAML::Emitter emitter;
            emitter << YAML::BeginMap;
            
            for (const auto& entry : pImpl->entries) {
                emitter << YAML::Key << entry.key << YAML::Value;
                std::visit([&emitter](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        emitter << val;
                    } else if constexpr (std::is_same_v<T, int>) {
                        emitter << val;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        emitter << static_cast<int>(val);
                    } else if constexpr (std::is_same_v<T, double>) {
                        emitter << val;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        emitter << (val ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        emitter << YAML::BeginSeq;
                        for (const auto& item : val) {
                            emitter << item;
                        }
                        emitter << YAML::EndSeq;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        emitter << YAML::BeginSeq;
                        for (const auto& item : val) {
                            emitter << item;
                        }
                        emitter << YAML::EndSeq;
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        emitter << YAML::BeginSeq;
                        for (const auto& item : val) {
                            emitter << item;
                        }
                        emitter << YAML::EndSeq;
                    } else {
                        emitter << "";
                    }
                }, entry.value);
            }
            
            emitter << YAML::EndMap;
            file << emitter.c_str();
            success = true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to save YAML file ") + filepath + ": " + e.what());
        }
    } else {
        // Save as key=value format for backward compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            std::visit([&file](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << val << "\"\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << val << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << val << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (val ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << val[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << val[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << val[i] << "\"";
                    }
                    file << "]\n";
                }
            }, entry.value);
        }
        success = true;
    }
    
    file.close();
    return success;
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
// Parse JSON data and populate configuration
bool Config::parseJSON(const nlohmann::json& jsonData) {
    try {
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it->key();
                setFromJSON(key, *it, ConfigSource::File);
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
    }
    return false;
}

// Parse YAML data and populate configuration
bool Config::parseYAML(const YAML::Node& yamlData) {
    try {
        if (yamlData.IsMap()) {
            for (const auto& node : yamlData) {
                std::string key = node.first.as<std::string>();
                setFromYAML(key, node.second, ConfigSource::File);
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
    }
    return false;
}

// Helper to set configuration value from JSON
void Config::setFromJSON(const std::string& key, const nlohmann::json& jsonValue, ConfigSource source) {
    if (jsonValue.is_string()) {
        set(key, jsonValue.get<std::string>(), source);
    } else if (jsonValue.is_number_integer()) {
        set(key, jsonValue.get<int>(), source);
    } else if (jsonValue.is_number_float()) {
        set(key, jsonValue.get<double>(), source);
    } else if (jsonValue.is_boolean()) {
        set(key, jsonValue.get<bool>(), source);
    } else if (jsonValue.is_array()) {
        // Handle arrays - for now, convert to vector<string>
        std::vector<std::string> strArray;
        for (const auto& item : jsonValue) {
            strArray.push_back(item.dump());
        }
        set(key, strArray, source);
    }
}

// Helper to set configuration value from YAML
void Config::setFromYAML(const std::string& key, const YAML::Node& yamlValue, ConfigSource source) {
    if (yamlValue.IsScalar()) {
        // Try to detect type from scalar
        std::string value = yamlValue.as<std::string>();
        
        // Check if it's a number
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.length()) {
                set(key, intVal, source);
                return;
            }
        } catch (...) {}
        
        try {
            double floatVal = std::stod(value);
            set(key, floatVal, source);
            return;
        } catch (...) {}
        
        // Check if it's a boolean
        if (value == "true" || value == "false" || value == "yes" || value == "no" || 
            value == "on" || value == "off") {
            bool boolVal = (value == "true" || value == "yes" || value == "on");
            set(key, boolVal, source);
            return;
        }
        
        // Otherwise treat as string
        set(key, value, source);
    } else if (yamlValue.IsSequence()) {
        // Handle sequences - for now, convert to vector<string>
        std::vector<std::string> strArray;
        for (const auto& item : yamlValue) {
            strArray.push_back(item.as<std::string>());
        }
        set(key, strArray, source);
    }
}

// Explicit template instantiations


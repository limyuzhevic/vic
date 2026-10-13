#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

using json = nlohmann::json;
using YAML::Node;

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
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".json5" || ext == ".jsonc") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for compatibility
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            ++lineNum;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::stringnpos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key found in config file " + filepath + " at line " + std::to_string(lineNum));
                    continue;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Invalid line format in config file " + filepath + " at line " + std::to_string(lineNum) + ": " + line);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
        return true;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
            return false;
        }
        
        json configJson;
        file >> configJson;
        
        // Process JSON object recursively
        for (auto it = configJson.begin(); it != configJson.end(); ++it) {
            std::string key = it.key();
            ConfigSource source = ConfigSource::File;
            std::string description = "";
            
            // Extract metadata if available
            auto keyObj = configJson[key];
            if (keyObj.is_object() && keyObj.contains("_metadata")) {
                auto metadata = keyObj["_metadata"];
                if (metadata.contains("description")) {
                    description = metadata["description"].get<std::string>();
                }
                if (metadata.contains("source")) {
                    std::string sourceStr = metadata["source"].get<std::string>();
                    if (sourceStr == "command_line") source = ConfigSource::CommandLine;
                    else if (sourceStr == "runtime") source = ConfigSource::Runtime;
                }
            }
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            auto& jsonValue = keyObj.is_object() ? keyObj.value() : keyObj;
            
            if (jsonValue.is_string()) {
                value = jsonValue.get<std::string>();
            } else if (jsonValue.is_number_integer()) {
                value = jsonValue.get<int64_t>();
            } else if (jsonValue.is_number_unsigned()) {
                value = static_cast<int64_t>(jsonValue.get<uint64_t>());
            } else if (jsonValue.is_number_float()) {
                value = jsonValue.get<double>();
            } else if (jsonValue.is_boolean()) {
                value = jsonValue.get<bool>();
            } else if (jsonValue.is_array()) {
                // Handle array types - determine element type
                auto arr = jsonValue.get<std::vector<json>>();
                if (!arr.empty()) {
                    if (arr[0].is_number_integer()) {
                        std::vector<int64_t> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<int64_t>());
                        }
                        value = vec;
                    } else if (arr[0].is_number_float()) {
                        std::vector<double> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<double>());
                        }
                        value = vec;
                    } else if (arr[0].is_string()) {
                        std::vector<std::string> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<std::string>());
                        }
                        value = vec;
                    }
                }
            }
            
            set(key, value, source);
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error parsing JSON config file ") + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node configNode = YAML::LoadFile(filepath);
        
        // Process YAML mapping recursively
        for (const auto& node : configNode) {
            std::string key = node.first.as<std::string>();
            ConfigSource source = ConfigSource::File;
            std::string description = "";
            
            // Check for metadata
            auto valueNode = node.second;
            if (valueNode.IsMap() && valueNode["_metadata"]) {
                auto metadata = valueNode["_metadata"];
                if (metadata["description"]) {
                    description = metadata["description"].as<std::string>();
                }
                if (metadata["source"]) {
                    std::string sourceStr = metadata["source"].as<std::string>();
                    if (sourceStr == "command_line") source = ConfigSource::CommandLine;
                    else if (sourceStr == "runtime") source = ConfigSource::Runtime;
                }
            }
            
            // Remove metadata before processing
            auto jsonValue = valueNode;
            if (jsonValue.IsMap() && jsonValue["_metadata"]) {
                // Convert YAML to JSON equivalent
                YAML::Emitter out;
                out << valueNode;
                std::string yamlStr = out.str();
                jsonValue = json::parse(yamlStr);
            }
            
            // Convert YAML value to ConfigValue
            ConfigValue value;
            if (jsonValue.IsString()) {
                value = jsonValue.get<std::string>();
            } else if (jsonValue.IsInt()) {
                value = jsonValue.get<int64_t>();
            } else if (jsonValue.IsNumber()) {
                value = jsonValue.get<double>();
            } else if (jsonValue.IsBoolean()) {
                value = jsonValue.get<bool>();
            } else if (jsonValue.IsArray()) {
                // Handle array types
                std::vector<json> arr;
                for (const auto& elem : jsonValue) {
                    arr.push_back(elem);
                }
                
                if (!arr.empty()) {
                    if (arr[0].is_number_integer()) {
                        std::vector<int64_t> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<int64_t>());
                        }
                        value = vec;
                    } else if (arr[0].is_number_float()) {
                        std::vector<double> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<double>());
                        }
                        value = vec;
                    } else if (arr[0].is_string()) {
                        std::vector<std::string> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<std::string>());
                        }
                        value = vec;
                    }
                }
            }
            
            set(key, value, source);
        }
        
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error parsing YAML config file ") + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return saveToJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return saveToYAML(filepath);
        } else {
            // Fallback to simple key=value format for compatibility
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
                return false;
            }
            
            file << "# NLM Configuration File\n";
            file << "# Generated by NLM on " << std::chrono::system_clock::now() << "\n\n";
            
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                // Format value based on type
                std::visit([&file](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << val << "\"\n";
                    } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (val ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
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
                    } else {
                        file << entry.key << " = " << val << "\n";
                    }
                }, entry.value);
                file << "\n";
            }
            
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving config file ") + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        json configJson;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            // Add metadata for special handling
            if (entry.source == ConfigSource::CommandLine) {
                configJson[key]["_metadata"]["source"] = "command_line";
            } else if (entry.source == ConfigSource::Runtime) {
                configJson[key]["_metadata"]["source"] = "runtime";
            }
            
            if (!entry.description.empty()) {
                configJson[key]["_metadata"]["description"] = entry.description;
            }
            
            // Convert ConfigValue to JSON
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    configJson[key] = val;
                } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                    configJson[key] = val;
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    configJson[key] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    configJson[key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    configJson[key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    configJson[key] = val;
                } else {
                    configJson[key] = val;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file for writing: " + filepath);
            return false;
        }
        
        file << configJson.dump(2);  // Pretty print with 2-space indent
        
        NLM_LOG_INFO("Saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error writing JSON config file ") + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter out;
        
        out << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            out << YAML::Key << key;
            out << YAML::Value;
            
            // Convert ConfigValue to YAML
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    out << YAML::FlowSeq;
                    for (const auto& elem : val) {
                        out << elem;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    out << YAML::FlowSeq;
                    for (const auto& elem : val) {
                        out << elem;
                    }
                } else {
                    out << val;
                }
            }, entry.value);
        }
        
        out << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML config file for writing: " + filepath);
            return false;
        }
        
        file << out.c_str();
        
        NLM_LOG_INFO("Saved YAML configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error writing YAML config file ") + filepath + ": " + e.what());
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
